#include "browser.h"

DcmFileFormat GetDicomFileFromCollection(std::vector<DicomPatient> &collection, CollectionIndex index)
{
    return collection[index.patient_index].dicom_studies[index.study_index].dicom_series[index.series_index].dicom_files[index.file_index];
}

void LoadDicomFiles(char *path, std::vector<DicomPatient> &collection, bool recursive)
{
    if (!recursive)
    {
        for (auto &p : std::filesystem::directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
        {
            if (p.path().extension() == ".dcm")
                LoadDicomFileIntoCollection(p.path().string().c_str(), collection);
        }
    }
    else
    {
        for (auto &p : std::filesystem::recursive_directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
        {
            if (p.path().extension() == ".dcm")
                LoadDicomFileIntoCollection(p.path().string().c_str(), collection);
        }
    }

    // Debug Code
    DebugDicomFileScan(collection);
}

void LoadDicomFileIntoCollection(OFFilename file_name, std::vector<DicomPatient> &collection)
{
    // Load all the files
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(file_name);

    if (!status.good())
        std::cout << "error opening dicom file" << std::endl;

    OFString patient_name;
    OFString study_id;
    OFString series_id;
    status = fileformat.getDataset()->findAndGetOFString(DCM_PatientName, patient_name);
    status = fileformat.getDataset()->findAndGetOFString(DCM_StudyID, study_id);
    status = fileformat.getDataset()->findAndGetOFString(DCM_SeriesNumber, series_id);

    if (!status.good())
        std::cout << "error reading main DICOM tags" << std::endl;

    if (collection.empty())
    {
        // Fill the collection with the first ever entry
        DicomPatient new_patient;
        new_patient.patient_name = patient_name;
        DicomStudy new_study;
        new_study.study_id = study_id;
        DicomSeries new_series;
        new_series.series_id = series_id;

        new_series.dicom_files.push_back(fileformat);
        new_study.dicom_series.push_back(new_series);
        new_patient.dicom_studies.push_back(new_study);
        collection.push_back(new_patient);
    }
    else
    { // Check for previous entries
        for (int i = 0; i < collection.size(); i++)
        {
            if (collection[i].patient_name == patient_name)
            {
                for (int j = 0; j < collection[i].dicom_studies.size(); j++)
                {
                    if (collection[i].dicom_studies[j].study_id == study_id)
                    {
                        for (int k = 0; k < collection[i].dicom_studies[j].dicom_series.size(); k++)
                        {
                            if (collection[i].dicom_studies[j].dicom_series[k].series_id == series_id)
                            {
                                // To avoid adding duplicate files if the same folder is scanned again,
                                // we just check if the instance number exists, which should be unique to this
                                // dicom series

                                bool exists = false;
                                for (auto &file : collection[i].dicom_studies[j].dicom_series[k].dicom_files)
                                {
                                    OFString instance_number;
                                    file.getDataset()->findAndGetOFString(DCM_InstanceNumber, instance_number);

                                    OFString new_instance_number;
                                    fileformat.getDataset()->findAndGetOFString(DCM_InstanceNumber, new_instance_number);

                                    if (instance_number == new_instance_number)
                                        exists = true;
                                }

                                if (!exists)
                                    collection[i].dicom_studies[j].dicom_series[k].dicom_files.push_back(fileformat);
                            }
                            else
                            { // Make a new Series
                                DicomSeries new_series;
                                new_series.series_id = series_id;
                                new_series.dicom_files.push_back(fileformat);
                                collection[i].dicom_studies[j].dicom_series.push_back(new_series);
                            }
                        }
                    }
                    else
                    { // Make a new Study
                        DicomStudy new_study;
                        new_study.study_id = study_id;
                        DicomSeries new_series;
                        new_series.series_id = series_id;
                        new_series.dicom_files.push_back(fileformat);
                        collection[i].dicom_studies.push_back(new_study);
                    }
                }
            }
            else
            { // Make a new patient group
                DicomPatient new_patient;
                new_patient.patient_name = patient_name;
                DicomStudy new_study;
                new_study.study_id = study_id;
                DicomSeries new_series;
                new_series.series_id = series_id;

                new_series.dicom_files.push_back(fileformat);
                new_study.dicom_series.push_back(new_series);
                new_patient.dicom_studies.push_back(new_study);
                collection.push_back(new_patient);
            }
        }
    }
}

/* Iterate over the dicom_collection nested vectors to debug how many files were found */
void DebugDicomFileScan(const std::vector<DicomPatient> &dicom_collection)
{
    std::cout << "Found " << dicom_collection.size() << " patients" << std::endl;

    for (int i = 0; i < dicom_collection.size(); i++)
    {
        std::cout << "      ";
        std::cout << "Patient " << i << " has " << dicom_collection[i].dicom_studies.size() << " studies" << std::endl;

        for (int j = 0; j < dicom_collection[i].dicom_studies.size(); j++)
        {
            std::cout << "              ";
            std::cout << "Study " << j << " has " << dicom_collection[i].dicom_studies[j].dicom_series.size() << " series" << std::endl;

            for (int k = 0; k < dicom_collection[i].dicom_studies[j].dicom_series.size(); k++)
            {
                std::cout << "                      ";
                std::cout << "Series " << k << " has " << dicom_collection[i].dicom_studies[j].dicom_series[k].dicom_files.size() << " files" << std::endl;
            }
        }
    }
}
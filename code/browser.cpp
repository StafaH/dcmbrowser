#include "browser.h"

DcmFileFormat GetDicomFileFromCollection(std::vector<DicomPatient> &collection, CollectionIndex index)
{
    return collection[index.patient_index].dicom_studies[index.study_index].dicom_series[index.series_index].dicom_files[index.file_index].file;
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
        DicomFile new_file;
        new_file.file = fileformat;
        new_file.path_tofile = file_name.getCharPointer();

        new_series.dicom_files.push_back(new_file);
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
                                for (auto &dicom_file : collection[i].dicom_studies[j].dicom_series[k].dicom_files)
                                {
                                    OFString instance_number;
                                    dicom_file.file.getDataset()->findAndGetOFString(DCM_InstanceNumber, instance_number);

                                    OFString new_instance_number;
                                    fileformat.getDataset()->findAndGetOFString(DCM_InstanceNumber, new_instance_number);

                                    if (instance_number == new_instance_number)
                                        exists = true;
                                }

                                DicomFile new_file;
                                new_file.file = fileformat;
                                new_file.path_tofile = file_name.getCharPointer();
                                if (!exists)
                                    collection[i].dicom_studies[j].dicom_series[k].dicom_files.push_back(new_file);
                            }
                            else
                            { // Make a new Series
                                DicomSeries new_series;
                                new_series.series_id = series_id;
                                DicomFile new_file;
                                new_file.file = fileformat;
                                new_file.path_tofile = file_name.getCharPointer();
                                new_series.dicom_files.push_back(new_file);
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
                        DicomFile new_file;
                        new_file.file = fileformat;
                        new_file.path_tofile = file_name.getCharPointer();
                        new_series.dicom_files.push_back(new_file);
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
                DicomFile new_file;
                new_file.file = fileformat;
                new_file.path_tofile = file_name.getCharPointer();

                new_series.dicom_files.push_back(new_file);
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

void LoadDicomImageFromPath(char *path)
{
    //DicomImage(path);
}

DicomImageTexture LoadImageFromDicomFile(std::vector<DicomPatient> &collection, CollectionIndex index)
{
    DicomImageTexture dicom_texture;
    DcmFileFormat file = GetDicomFileFromCollection(collection, index);
    DcmFileFormat *file_ptr = &file;

    DicomImage image(file_ptr, EXS_Unknown);

    if (image.getStatus() == EIS_Normal)
    {
        int image_height = image.getHeight();
        int image_width = image.getWidth();

        if (image.isMonochrome())
        {
            image.setMinMaxWindow();

            Uint8 *pixelData = (Uint8 *)(image.getOutputData(8 /* bits */));

            if (pixelData != NULL)
            {
                GLuint texture_id;
                glGenTextures(1, &texture_id);
                glBindTexture(GL_TEXTURE_2D, texture_id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

                // Use GL_LUMINANCE because dicom files are greyscale 8-bit
                glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, image_width, image_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixelData);

                dicom_texture.height = image_height;
                dicom_texture.width = image_width;
                dicom_texture.texture_id = (void *)(intptr_t)texture_id;

                //glDeleteTextures(1, &dicom_texture);
            }
        }
    }
    else
    {
        std::cerr << "Error: cannot load DICOM image (" << DicomImage::getString(image.getStatus()) << ")" << std::endl;
    }

    return dicom_texture;
}

const char *LoadDicomTag(std::vector<DicomPatient> &collection, CollectionIndex index, DcmTagKey tag)
{
    DcmFileFormat file = GetDicomFileFromCollection(collection, index);
    OFString value;
    OFCondition status = file.getDataset()->findAndGetOFString(tag, value);
    if (!status.good())
        value = "Unable to find this tag";
    return value.c_str();
}

void CopyAndAnonymizeCollection(char *path, std::vector<DicomPatient> &collection, bool anonymize)
{

    // Tags to clear for anonymization and what values to put
    std::vector<DcmTagKey> anon_tags = {DCM_PatientID,
                                        DCM_PatientBirthDate,
                                        DCM_InstitutionName,
                                        DCM_InstitutionAddress,
                                        DCM_AccessionNumber,
                                        DCM_ReferringPhysicianName,
                                        DCM_ReferringPhysicianAddress,
                                        DCM_OperatorsName};

    // Generate the root directory first from the user chosen path
    std::string path_string = path;
    std::filesystem::path root_path(path_string);
    if (!std::filesystem::exists(root_path))
    {
        std::filesystem::create_directory(root_path);
    }

    for (int i = 0; i < collection.size(); i++)
    {
        // Patient Name Directories = root path + patient name
        std::string patient_name = collection[i].patient_name.c_str();
        std::string patient_path = path_string + "/" + patient_name;

        if (!std::filesystem::exists(patient_path))
        {
            std::filesystem::create_directory(patient_path);
        }

        for (int j = 0; j < collection[i].dicom_studies.size(); j++)
        {
            // Study Directories = patient path + study name
            std::string study_name = collection[i].dicom_studies[j].study_id.c_str();
            std::string study_path = patient_path + "/" + study_name;

            if (!std::filesystem::exists(study_path))
            {
                std::filesystem::create_directory(study_path);
            }

            for (int k = 0; k < collection[i].dicom_studies[j].dicom_series.size(); k++)
            {
                // Series Directories = study path + series name
                std::string series_name = collection[i].dicom_studies[j].dicom_series[k].series_id.c_str();
                std::string series_path = study_path + "/" + series_name;

                if (!std::filesystem::exists(series_path))
                {
                    std::filesystem::create_directory(series_path);
                }

                // Create the anonymous file directory if enabled
                if (anonymize)
                {
                    std::string anonymous_series_path = series_path + "_anonymous";
                    if (!std::filesystem::exists(anonymous_series_path))
                    {
                        std::filesystem::create_directory(anonymous_series_path);
                    }
                }

                for (int l = 0; l < collection[i].dicom_studies[j].dicom_series[k].dicom_files.size(); l++)
                {
                    // File Directories = series path + file name + .dcm
                    DcmFileFormat file = collection[i].dicom_studies[j].dicom_series[k].dicom_files[l].file;
                    OFString file_name;
                    file.getDataset()->findAndGetOFString(DCM_InstanceNumber, file_name);
                    std::string file_name_string = file_name.c_str();
                    std::string new_file_path = series_path + "/" + file_name_string + ".dcm";

                    if (!std::filesystem::exists(new_file_path))
                    {
                        std::string old_file_path = collection[i].dicom_studies[j].dicom_series[k].dicom_files[l].path_tofile;
                        std::filesystem::copy_file(old_file_path, new_file_path);
                    }

                    // For anonymizing, we copy all files into a new directory marked with anonymous,
                    // and replace key tags that would provide patient/staff information
                    if (anonymize)
                    {
                        std::string anonymous_series_path = series_path + "_anonymous";
                        std::string anonymouse_file_path = anonymous_series_path + "/" + file_name_string + ".dcm";

                        if (!std::filesystem::exists(anonymouse_file_path))
                        {
                            std::string old_file_path = collection[i].dicom_studies[j].dicom_series[k].dicom_files[l].path_tofile;
                            std::filesystem::copy_file(old_file_path, anonymouse_file_path);
                        }

                        DcmFileFormat anon_file;
                        anon_file.loadFile(anonymouse_file_path.c_str());

                        for (auto &tag : anon_tags)
                        {
                            anon_file.getDataset()->putAndInsertString(tag, "Anonymous", true);
                        }
                    }
                }
            }
        }
    }
}
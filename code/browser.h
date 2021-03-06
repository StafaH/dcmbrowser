#pragma once

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"

#include "GL/glew.h"
#include "imgui/imgui.h"
#include <filesystem>
#include <vector>
#include <string>
#include <unordered_map>

/*
Dicom Grouping Hierarchy
- Patient Name
    - Study ID 
        - Series ID
*/
struct DicomFile
{
    DcmFileFormat file;
    std::string path_tofile;
};


struct DicomSeries
{
    OFString series_id;
    std::vector<DicomFile> dicom_files;
    
    // Cache of the directory path that has these dicom files came from
    // currently not used, could be useful later
    std::string directory_path;
};

struct DicomStudy
{
    OFString study_id;
    std::vector<DicomSeries> dicom_series;
};

struct DicomPatient
{
    OFString patient_name;
    std::vector<DicomStudy> dicom_studies;
};

// Index to store which dicom file is selected
// in the tree view
struct CollectionIndex
{
    CollectionIndex(int patient_i, int study_i, int series_i, int file_i) : patient_index(patient_i), study_index(study_i), series_index(series_i), file_index(file_i) {}

    CollectionIndex() : patient_index(0), study_index(0), series_index(0), file_index(0) {}

    // Returns wether the gives index values are the same as the index ones in the index struct
    bool DoesIndexMatch(int patient_i, int study_i, int series_i, int file_i)
    {
        return (patient_index == patient_i && study_index == study_i && series_index == series_i && file_index == file_i);
    }
    
    int patient_index = 0;
    int study_index = 0;
    int series_index = 0;
    int file_index = 0;
};

struct DicomImageTexture
{
    void* texture_id;
    int height;
    int width;
};

//////////////////////
/* Helper Functions */
//////////////////////

// Returns the dicom file at a given index
DcmFileFormat GetDicomFileFromCollection(std::vector<DicomPatient>& collection, CollectionIndex index);

// Load Dicom files from directory path into collection
void LoadDicomFiles(char *path, std::vector<DicomPatient>& collection, bool recursive);

// Load individual Dicom file into the collection - used by LoadDicomFiles
void LoadDicomFileIntoCollection(OFFilename file_name, std::vector<DicomPatient>& collection);

// Debug the number of files in the collection
void DebugDicomFileScan(const std::vector<DicomPatient>& dicom_collection);

// Unused
void LoadDicomImageFromPath(char *path);

// Generates an OpenGL texture and renders using ImGui call
DicomImageTexture LoadImageFromDicomFile(std::vector<DicomPatient>& collection, CollectionIndex index);

// Load a single tag from a dicom file found in the collection
const char* LoadDicomTag(std::vector<DicomPatient>& collection, CollectionIndex index, DcmTagKey tag);

// Copy the collection into structured directories, enabling anonymize will copy and then create 
// a second copy with the anonymouse file
void CopyAndAnonymizeCollection(char* path, std::vector<DicomPatient>& collection, bool anonymize);



/****************************************************************************
  text_constants.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

const wchar_t ERROR_INVALID_STRUCTURE[] = L"Error has occurred,\nthe data structure might be damaged.";
const wchar_t ERROR_MSG[] = L"Error";
const wchar_t ERROR_NO_CODE[] = L"No code";
const wchar_t ERROR_EMPTY_FILE_NAME[] = L"Empty file name";
const wchar_t ERROR_FOLDER_CREATE_OR_ACCESS[] = L"Failed to create or access the folder:";
const wchar_t ERROR_FOLDER_ACCESS[] = L"Failed to access the folder:";
const wchar_t ERROR_IO_FILE[] = L"File I/O operation failed:";
const wchar_t ERROR_FORCE_THREAD_TERMINATION[] = L"The thread has been forcedly terminated";
const wchar_t ERROR_FAILED_TERMINATE_THREAD[] = L"Serious failure - unable to terminate the thread forcedly";
const wchar_t ERROR_FAILED_CLOSE_HANDLE[] = L"Failed to close a handle";
const wchar_t ERROR_RUNTIME[] = L"Runtime ERROR!";
const wchar_t ERROR_UNSUPPORTED_DATA_FORMAT[] = L"Unsupported data format";
const wchar_t EMPTY_DOCUMENT[] = L"Empty document";
const wchar_t NOT_IMPLEMENTED[] = L"Not implemented";
const wchar_t WANT_DEFAULT_PARAM[] = L"Are you really want to set default parameters?";
const wchar_t ERROR_LOAD_PARAMETERS_FROM_FILE[] = L"Failed to load parameters from a file";
const wchar_t ERROR_SAVE_PARAMETERS_IN_FILE[] = L"Failed to save parameters in a file";

const wchar_t AVI_FILTER[] = L"Video Files (*.avi)|*.avi|All Files (*.*)|*.*||";
const wchar_t BMP_FILTER[] = L"Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||";
const wchar_t XML_FILTER[] = L"XML Files (*.xml)|*.xml|All Files (*.*)|*.*||";
const wchar_t HTML_FILTER[] = L"HTML Files (*.htm;*.html)|*.htm;*.html|All Files (*.*)|*.*||";
const wchar_t PARAM_FILTER[] = L"Parameter Files (*.param)|*.param|All Files (*.*)|*.*||";

const wchar_t WILDCARD_BMP[] = L"*.bmp";
const wchar_t WILDCARD_XML[] = L"*.xml";
const wchar_t WILDCARD_CSS[] = L"*.css";
const wchar_t WILDCARD_AVI[] = L"*.avi";
const wchar_t WILDCARD_HTML[] = L"*.html";
const wchar_t WILDCARD_HTM[] = L"*.htm";
const wchar_t WILDCARD_GIF[] = L"*.gif";
const wchar_t WILDCARD_DLL[] = L"*.dll";
const wchar_t WILDCARD_PARAM[] = L"*.param";

const wchar_t TESTVIDEO_DIR[] = L"TestVideo";
const wchar_t TESTVIDEO_INI[] = L"testvideo.ini";

const wchar_t MONTH[1+12][4] = {{L"nul"},{L"jan"},{L"feb"},{L"mar"},{L"apr"},{L"may"},{L"jun"},
                                         {L"jul"},{L"aug"},{L"sep"},{L"oct"},{L"nov"},{L"dec"}};

const char    A_LINE_SEPARATOR[] = "-----------------------------------";


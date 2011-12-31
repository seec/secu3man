cmake_minimum_required(VERSION 2.6) 

ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)

set(PROJECT secu3man)  
project(${PROJECT}) 

set(LIBRARIES about fwimpexp hexutils hiscctrl io-core midesk
  paramdesk propgrid tabldesk ui-core
  )
 
foreach(LIBRARY ${LIBRARIES})
  add_subdirectory(../${LIBRARY} ../${LIBRARY})
endforeach()

#set(EXECUTABLE_OUTPUT_PATH ../../binaries) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)

set(CMAKE_MFC_FLAG 2)  
include_directories(../)
include_directories(../secu3man)
 
set(HEADERS 
  Application/StdAfx.h
  Application/secu3man.h
  Application/ISECU3man.h
  Application/CommunicationManager.h
  MainFrame/ChildView.h
  MainFrame/ChildViewManager.h
  MainFrame/MainFrame.h
  MainFrame/MainFrameController.h
  MainFrame/MainFrameManager.h
  MainFrame/StatusBarManager.h
  Settings/AppSettingsController.h
  Settings/AppSettingsDlg.h
  Settings/AppSettingsManager.h
  Settings/AppSettingsModel.h
  Settings/IAppSettingsDlg.h
  Settings/ISettingsData.h
  TabControllers/CheckEngineTabController.h
  TabControllers/FirmwareTabController.h
  TabControllers/KnockChannelTabcontroller.h
  Tabcontrollers/LogPlayerTabController.h
  TabControllers/ParamMonTabController.h
  TabControllers/PMControllerBase.h
  TabControllers/PMInitDataCollector.h
  TabControllers/PMMonitorController.h
  TabControllers/PMParamsController.h
  TabControllers/PMTablesController.h
  TabControllers/TabcontrollersCommunicator.h
  TabDialogs/CheckEngineTabDlg.h
  TabDialogs/FirmwareContextMenuManager.h
  TabDialogs/FirmwareTabDlg.h
  TabDialogs/KnockChannelTabDlg.h
  TabDialogs/LogPlayerTabDlg.h
  TabDialogs/LPControlPanelDlg.h
  TabDialogs/ParamMonTabDlg.h
  TabsManagement/ITabController.h
  TabsManagement/MainTabController.h
  TabsManagement/MainTabManager.h
  Resources/Resource.h
  )
 
set(SOURCES 
  Application/StdAfx.cpp
  Application/secu3man.cpp
  Application/CommunicationManager.cpp
  Application/CommunicationManager.cpp
  MainFrame/ChildView.cpp
  MainFrame/ChildViewManager.cpp
  MainFrame/MainFrame.cpp
  MainFrame/MainFrameController.cpp
  MainFrame/MainFrameManager.cpp
  MainFrame/StatusBarManager.cpp
  Settings/AppSettingsController.cpp
  Settings/AppSettingsDlg.cpp
  Settings/AppSettingsManager.cpp
  Settings/AppSettingsModel.cpp
  TabControllers/CheckEngineTabController.cpp
  TabControllers/FirmwareTabController.cpp
  TabControllers/KnockChannelTabcontroller.cpp
  Tabcontrollers/LogPlayerTabController.cpp
  TabControllers/ParamMonTabController.cpp
  TabControllers/PMInitDataCollector.cpp
  TabControllers/PMMonitorController.cpp
  TabControllers/PMParamsController.cpp
  TabControllers/PMTablesController.cpp
  TabControllers/TabcontrollersCommunicator.cpp
  TabDialogs/CheckEngineTabDlg.cpp
  TabDialogs/FirmwareContextMenuManager.cpp
  TabDialogs/FirmwareTabDlg.cpp
  TabDialogs/KnockChannelTabDlg.cpp
  TabDialogs/LogPlayerTabDlg.cpp
  TabDialogs/LPControlPanelDlg.cpp
  TabDialogs/ParamMonTabDlg.cpp
  TabsManagement/MainTabController.cpp
  TabsManagement/MainTabManager.cpp
  Resources/secu3man.rc
  ) 

foreach(src_file ${SOURCES})
  set_source_files_properties(${src_file}
  PROPERTIES COMPILE_FLAGS "/YuStdAfx.h"
  )
endforeach(src_file ${SOURCES})
set_source_files_properties(Application/StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")
 
add_executable(${PROJECT} WIN32 ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ${LIBRARIES})
// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "EditorToolbar.h"
#include "EditorToolbarStyle.h"
#include "EditorToolbarCommands.h"
#include "Misc/MessageDialog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "AssetToolsModule.h"
#include "JsonUtilities.h"
#include "Json.h"
#include "LevelEditor.h"
#include "AssetImportTask.h"
#include "Factories/FbxImportUI.h"
static const FName EditorToolbarTabName("EditorToolbar");

#define LOCTEXT_NAMESPACE "FEditorToolbarModule"

void FEditorToolbarModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FEditorToolbarStyle::Initialize();
	FEditorToolbarStyle::ReloadTextures();

	FEditorToolbarCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FEditorToolbarCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FEditorToolbarModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FEditorToolbarModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FEditorToolbarModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FEditorToolbarModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorToolbarStyle::Shutdown();

	FEditorToolbarCommands::Unregister();
}

void FEditorToolbarModule::PluginButtonClicked()
{

	// Put your "OnButtonClicked" stuff here
	//FText DialogText = FText::Format(
	//						LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
	//						FText::FromString(TEXT("FEditorToolbarModule::PluginButtonClicked()")),
	//						FText::FromString(TEXT("EditorToolbar.cpp"))
	//				   );
	//FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	TArray<FString> ImportFiles;
	ImportFiles.Add("D:\\xujie\\untitled.fbx");
	FString RootDestinationPath = "/Game/skeletonmesh";
	TArray<TPair<FString, FString>> FilesAndDestinations;
	AssetToolsModule.Get().ExpandDirectories(ImportFiles, RootDestinationPath, FilesAndDestinations);
    //AssetToolsModule.Get().ImportAssets(ImportFiles, RootDestinationPath, nullptr, true, &FilesAndDestinations);
	TSharedPtr<FJsonObject> ImportGroupJsonData = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> ImportSettings = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> ImportGroups = MakeShareable(new FJsonObject);

	ImportSettings->SetNumberField("OriginalImportType", 2);
	ImportSettings->SetNumberField("MeshTypeToImport",1);
	ImportGroupJsonData->SetObjectField("ImportSettings", ImportSettings);
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(ImportGroupJsonData.ToSharedRef(), Writer);
	UE_LOG(LogTemp, Warning, TEXT("resulting jsonString -> %s"), *OutputString);
	FString jsonfilepath = FPaths::ProjectSavedDir().Append("json.ini");
	FString content;
	FFileHelper::LoadFileToString(content, *jsonfilepath);
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(content);
	FJsonSerializer::Deserialize(Reader,ImportGroups);

	UAutomatedAssetImportData *aaid = NewObject<UAutomatedAssetImportData>();
	aaid->Filenames = ImportFiles;
	aaid->DestinationPath = RootDestinationPath;
	aaid->ImportGroupJsonData = ImportGroups;
	//AssetToolsModule.Get().ImportAssetsAutomated(aaid);

	UAssetImportTask* ait = NewObject<UAssetImportTask>();
	ait->Filename = "D:\\xujie\\untitled.fbx";
	ait->DestinationPath = "/Game/skeletonmesh";
	ait->DestinationName = "123";
	ait->bReplaceExisting = true;
	ait->bAutomated = true;
	ait->bSave = true;
	UFbxImportUI * Fbxui = NewObject<UFbxImportUI>();
	Fbxui->bImportAsSkeletal = true;
	Fbxui->MeshTypeToImport = (EFBXImportType)1;
	Fbxui->OriginalImportType = (EFBXImportType)1;
	Fbxui->bAutomatedImportShouldDetectType = false;
	ait->Options = Fbxui;
	TArray<UAssetImportTask*>aitarray;
	aitarray.Add(ait);
	AssetToolsModule.Get().ImportAssetTasks(aitarray);
}

void FEditorToolbarModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FEditorToolbarCommands::Get().PluginAction);
}

void FEditorToolbarModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FEditorToolbarCommands::Get().PluginAction);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEditorToolbarModule, EditorToolbar)
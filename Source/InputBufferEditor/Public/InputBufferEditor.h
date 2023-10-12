// Copyright 2018 Isaac Hsu.

#pragma once

#include "Engine.h"
#include "UnrealEd.h"
#include "AssetTypeCategories.h"
#include "IAssetTools.h"
#include "IAssetTypeActions.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(InputBufferEditorLog, All, All)

class FInputBufferEditorModule : public IModuleInterface
{
public:

	FInputBufferEditorModule();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	/** All created asset type actions.  Cached here so that we can unregister them during shutdown. */
	TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;

	EAssetTypeCategories::Type AssetCategoryBit;

private:

	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);
};
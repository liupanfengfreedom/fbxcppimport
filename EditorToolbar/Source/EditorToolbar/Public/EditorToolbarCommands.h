// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorToolbarStyle.h"

class FEditorToolbarCommands : public TCommands<FEditorToolbarCommands>
{
public:

	FEditorToolbarCommands()
		: TCommands<FEditorToolbarCommands>(TEXT("EditorToolbar"), NSLOCTEXT("Contexts", "EditorToolbar", "EditorToolbar Plugin"), NAME_None, FEditorToolbarStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};

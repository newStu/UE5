// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyThirdGameMode.h"
#include "MyThirdCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMyThirdGameMode::AMyThirdGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

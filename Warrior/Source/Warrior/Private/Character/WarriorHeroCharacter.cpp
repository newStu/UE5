// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WarriorHeroCharacter.h"
#include "WarriorDebugHelper.h"

void AWarriorHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Debug::Print(TEXT("Hello, Working")); 
	
}

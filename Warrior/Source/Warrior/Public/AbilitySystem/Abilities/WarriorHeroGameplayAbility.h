// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h" 
#include "WarriorHeroGameplayAbility.generated.h"

class UHeroCombatComponent;
class AWarriorHeroCharacter;
class AWarriorHeroPlayerController;
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorHeroGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()
	
private:
	TWeakObjectPtr<AWarriorHeroCharacter> CachedWarriorHeroCharacter;
	
	TWeakObjectPtr<AWarriorHeroPlayerController> CachedWarriorHeroController;
	
public:
	UFUNCTION(BlueprintPure, Category="Warrior|Ability")
	AWarriorHeroCharacter* GetHeroCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category="Warrior|Ability")
	AWarriorHeroPlayerController* GetHeroControllerFromActorInfo();
	
	UFUNCTION(BlueprintPure, Category="Warrior|Ability")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();
};

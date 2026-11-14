// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/WarriorBaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "WarriorHeroCharacter.generated.h"


struct FGameplayTag;
struct FInputActionValue;
class UDataAsset_InputConfig;
class SpringArmComponent;
class UCameraComponent;
class UHeroCombatComponent;



/**
 *
 */
UCLASS()
class WARRIOR_API AWarriorHeroCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	AWarriorHeroCharacter();
	
	
protected:
	// ~Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	// ~End APawn Interface
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
#pragma region Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent * CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent * FollowCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UHeroCombatComponent* HeroCombatComponent;
	
#pragma endregion
	
#pragma region Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	UDataAsset_InputConfig* InputConfigDataAsset;
	
	void Input_Move(const FInputActionValue& InputActionValue);
	
	void Input_Look(const FInputActionValue& InputActionValue);
	
	void Input_AbilityInputPressed(FGameplayTag InputPlayTag);
	void Input_AbilityInputReleased(FGameplayTag InputPlayTag);
#pragma endregion 
	
	
public:
	FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const { return  HeroCombatComponent; }
};
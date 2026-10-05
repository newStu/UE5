// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class MYTHIRD_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
	UPROPERTY(EditAnywhere)
	float testLen = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float testLenBlueprint = 0.f;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TestFunction();

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool GetTestBoolBlueprint();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void MyBlueprintEvent();

	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);

private: 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> PlayerCamera;
	
	
	// Input
	UPROPERTY(EditDefaultsOnly, Category= "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category= "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category= "Input")
	TObjectPtr<UInputAction> LookAction;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

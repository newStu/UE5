// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

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
	float testLen;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float testLenBlueprint;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TestFunction();

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool GetTestBoolBlueprint();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void MyBlueprintEvent();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

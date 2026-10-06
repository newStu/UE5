// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "SubPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MYTHIRD_API ASubPlayerCharacter : public APlayerCharacter
{
	GENERATED_BODY()
	
public:
	virtual void Attack() override;
	
	UPROPERTY(EditAnywhere)
	// TArray 学习
	TArray<int32> MyIntArray;
};

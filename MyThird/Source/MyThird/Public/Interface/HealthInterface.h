// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHealthInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYTHIRD_API IHealthInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Health")
	void Death();
	virtual void Death_Implementation() = 0;
	
	
	UFUNCTION(BlueprintNativeEvent, Category = "Health")
	void Damage();
	virtual void Damage_Implementation() = 0;
};

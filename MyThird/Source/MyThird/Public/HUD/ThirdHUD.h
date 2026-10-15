// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ThirdHUD.generated.h"

class UHealthWidget;
/**
 * 
 */
UCLASS()
class MYTHIRD_API AThirdHUD : public AHUD
{
	GENERATED_BODY()
	
	
public:
	// 保存Class
	UPROPERTY(EditDefaultsOnly, Category="HUD")
	TSubclassOf<UHealthWidget> HealthWidgetClass;
	
	// 保存实例
	UPROPERTY(EditDefaultsOnly, Category="HUD")
	TObjectPtr<UHealthWidget> HealthWidget;
	
	
	void CreateHealthWidget();
	
	void UpdateHealthWidget(float HealthPrecent);
};

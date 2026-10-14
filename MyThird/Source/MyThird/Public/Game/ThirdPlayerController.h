// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ThirdPlayerController.generated.h"


class UHealthWidget;
class URestartWidget;
class AThirdHUD;

/**
 * 
 */
UCLASS()
class MYTHIRD_API AThirdPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URestartWidget> ReStartWidgetClass;
	
	
	void ShowRestartWidget();
	
	void HideRestartWidget();
	
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHealthWidget> HealthWidgetClass;
	
	
	void UpdateHealthPercent(float HealthPercent);
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	TObjectPtr<URestartWidget> RestartWidget;
	
	UPROPERTY()
	TObjectPtr<UHealthWidget> HealthWidget;	
	
	UPROPERTY()
	TObjectPtr<AThirdHUD> ThirdHUD;
};

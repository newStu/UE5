// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

void AMyPlayerController::OnPossess(APawn* InPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("AMyPlayerController::OnPossess()"));
	
	Super::OnPossess(InPawn);
	
	if (InPawn != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn On Possess"));
		InPawn->SetActorLocation(FVector(0.f, 0.f, 0.f));
	}
}

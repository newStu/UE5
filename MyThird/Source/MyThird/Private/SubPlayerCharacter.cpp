// Fill out your copyright notice in the Description page of Project Settings.


#include "SubPlayerCharacter.h"

void ASubPlayerCharacter::Attack()
{
	Super::Attack();
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue,
			FString::Printf(TEXT("Child Attack from Character"))); 
	
	
	// TArray学习
	MyIntArray.Add(1);
	MyIntArray.Add(2);
	MyIntArray.Add(3);
	MyIntArray.Add(4);
	
	
	for (int32 i = 0; i < MyIntArray.Num(); ++i)
	{
		int32 element = MyIntArray[i];
		UE_LOG(LogTemp, Warning, TEXT("Index: %i; Element: %d"), i, element);
	}
	
	MyIntArray.Add(5);
	int32 ElementTwo = MyIntArray[4];
	UE_LOG(LogTemp, Warning, TEXT("新插入的Element: %d"), ElementTwo);
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SnakeGameMode.generated.h"

class ASnakePawn;
/**
 * 
 */
UCLASS()
class SNAKE3D_API ASnakeGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	void RegisterActor(AActor* Actor);
	
protected:
	virtual void BeginPlay() override;
	
private:
	UFUNCTION()
	void OnSneakHit(ASnakePawn* SnakePawn, AActor* OtherActor);
};

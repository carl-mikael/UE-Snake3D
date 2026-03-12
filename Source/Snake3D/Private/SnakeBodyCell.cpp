// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeBodyCell.h"


// Sets default values
ASnakeBodyCell::ASnakeBodyCell()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	SetRootComponent(MeshComponent);
}

// Called when the game starts or when spawned
void ASnakeBodyCell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASnakeBodyCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


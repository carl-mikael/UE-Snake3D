// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeBodyCell.h"


// Sets default values
ASnakeBodyCell::ASnakeBodyCell()
{
	UE_LOG(LogTemp, Log, TEXT("ASnakeBodyCell::ASnakeBodyCell()"));
	
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	SetRootComponent(MeshComponent);
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(this, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (IsValid(Mesh))
	{
		MeshComponent->SetStaticMesh(Mesh);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SnakeBodyCell::ASnakeBodyCell() Couldn't find mesh"));
	}
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


// Fill out your copyright notice in the Description page of Project Settings.


#include "Food.h"

// Sets default values
AFood::AFood()
{
	bReplicates = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	const FName MeshPath = TEXT("/Engine/BasicShapes/Sphere.Sphere");
	StaticMeshComponent->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(*MeshPath.ToString()).Object);
	StaticMeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
	SetRootComponent(StaticMeshComponent);
}

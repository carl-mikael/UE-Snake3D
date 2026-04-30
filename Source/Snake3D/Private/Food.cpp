// Fill out your copyright notice in the Description page of Project Settings.


#include "Food.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

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

void AFood::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (IsValid(NiagaraSystemComponent))
	{
		UNiagaraComponent* NiagaraSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NiagaraSystemComponent,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector::OneVector,
			true,
			true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AFood::EndPlay() - NiagaraSystemComponent is not valid!"));
	}
}
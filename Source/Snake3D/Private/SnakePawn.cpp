// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePawn.h"

#include "SnakeMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ASnakePawn::ASnakePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(DummyRoot);

	const FName HeadMeshAssetPath = TEXT("/Engine/BasicShapes/Cone.Cone");
	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
    HeadMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(*HeadMeshAssetPath.ToString()).Object);
	HeadMesh->SetNotifyRigidBodyCollision(true);
	HeadMesh->SetupAttachment(RootComponent);
	
	Movement = CreateDefaultSubobject<USnakeMovementComponent>(TEXT("Movement"));
	Movement->SetUpdatedComponent(RootComponent);
	
	CameraSpring = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpring"));
	CameraSpring->SetupAttachment(RootComponent);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpring);
	
	const FName BodyMeshAssetPath = TEXT("/Engine/BasicShapes/Cube.Cube");
	BodyCellMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(*BodyMeshAssetPath.ToString()).Object;
	
	NrOfBodyCells = 2;
	
	for (int i = 0; i < NrOfBodyCells; ++i)
	{
		const FString BodyCellName = FString::Printf(TEXT("BodyCell%d"), i);	
		UStaticMeshComponent* BodyCell = CreateDefaultSubobject<UStaticMeshComponent>(*BodyCellName);
		if (i == 0)
		{
			BodyCell->SetupAttachment(HeadMesh);
		}
		else
		{
			BodyCell->SetupAttachment(BodyCells[i - 1]);
		}
		
		BodyCells.Add(BodyCell);
	}
	
	BodyCellOffset = 100.0f;
	MovementSpeed = 100.0f;
}

void ASnakePawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	UE_LOG(LogTemp, Log, TEXT("SnakePawn::OnConstruction()!"));
}

// Called when the game starts or when spawned
void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Log, TEXT("SnakePawn::BeginPlay()!"));
	
	HeadMesh->OnComponentHit.AddUniqueDynamic(this, &ASnakePawn::OnHit);

	UStaticMesh* BodyMesh = BodyCellMesh.LoadSynchronous();
	if (IsValid(BodyMesh))
	{
		for (int i = 0; i < NrOfBodyCells; ++i)
		{
			UStaticMeshComponent* BodyCell = BodyCells[i];
			if (!IsValid(BodyCell))
			{
				UE_LOG(LogTemp, Error, TEXT("SnakePawn::BeginPlay() - Body cell invalid i: %i"), i);
				break;
			}
			BodyCell->SetStaticMesh(BodyMesh);
			const float XOffset = -BodyCellOffset * (i + 1);
			const FVector Offset = FVector(XOffset, 0.0f, 0.0f);
			BodyCell->AddLocalOffset(Offset);
		}
	}
}

void ASnakePawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	HeadMesh->OnComponentHit.RemoveDynamic(this, &ASnakePawn::OnHit);
}


void ASnakePawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                       FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Log, TEXT("SnakePawn::OnHit()"));
}

// Called every frame
void ASnakePawn::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UPawnMovementComponent* ASnakePawn::GetMovementComponent() const
{
	return Movement;
}

float ASnakePawn::GetMovementSpeed() const
{
	return MovementSpeed;
}


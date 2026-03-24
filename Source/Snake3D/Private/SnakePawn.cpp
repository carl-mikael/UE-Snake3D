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
	
	NrOfBodyCells = 2;
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
	
	if (IsValid(BodyCellActorClass))
	{
		for (int i = 0; i < NrOfBodyCells; ++i)
		{
			const FString BodyCellName = FString::Printf(TEXT("BodyCell%d"), i);	
			UChildActorComponent* ChildActorComponent = NewObject<UChildActorComponent>(this, *BodyCellName);
			ChildActorComponent->RegisterComponent();
			ChildActorComponent->SetChildActorClass(BodyCellActorClass);
			const AActor* ChildActor = ChildActorComponent->GetChildActor();
			if (!IsValid(ChildActor))
			{
				UE_LOG(LogTemp, Error, TEXT("SnakePawn::BeginPlay() - Failed to spawn body cell actor i: %i"), i);
				break;
			}
			
			BodyCellActors.Add(ChildActorComponent->GetChildActor());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SnakePawn - ChildActorClass ptr invalid"));
	}
	
	for (int i = 0; i < BodyCellActors.Num(); ++i)
	{
		const AActor* BodyCell = BodyCellActors[i];
		if (!IsValid(BodyCell))
		{
			UE_LOG(LogTemp, Error, TEXT("SnakePawn::BeginPlay() - Body cell invalid i: %i"), i);
			break;
		}
		
		const float XOffset = -BodyCellOffset * (i + 1);
		const FVector Offset = FVector(XOffset, 0.0f, 0.0f);
		const FVector BodyCellWorldLocation = GetRootComponent()->GetComponentLocation() + Offset;
		BodyCell->GetRootComponent()->SetWorldLocation(BodyCellWorldLocation);
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

	for (int i=0; i<BodyCellActors.Num(); ++i)
	{
		const AActor* BodyCell = BodyCellActors[i];
		if (!IsValid(BodyCell))
		{	
			UE_LOG(LogTemp, Error, TEXT("SnakePawn::Tick() - Body cell invalid i: %i"), i);
			break;
		}
		
		FVector PrevCellLocation = i==0? HeadMesh->GetComponentLocation() : BodyCellActors[i-1]->GetActorLocation();
		const FVector ToPrevCell = PrevCellLocation - BodyCell->GetActorLocation();
		const FRotator RotToPrevCell = ToPrevCell.Rotation();
		USceneComponent* CellRoot = BodyCell->GetRootComponent();
		CellRoot->SetWorldRotation(RotToPrevCell);
		FVector Move = FVector(MovementSpeed * DeltaTime, 0.0f, 0.0f);
		FVector NewLocation = BodyCell->GetActorLocation() + Move;
		const float MinDistanceSqr = MovementSpeed;
		const float DistanceSqr = (PrevCellLocation - NewLocation).Length();
		if (DistanceSqr < MinDistanceSqr)
		{
			Move.X -= MinDistanceSqr - DistanceSqr;
		}
		
		CellRoot->AddLocalOffset(Move);
	}
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


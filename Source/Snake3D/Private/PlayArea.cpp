// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayArea.h"

#include "Food.h"
#include "SnakeGameMode.h"
#include "Components/CapsuleComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APlayArea::APlayArea()
{
	bReplicates = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);

	FloorMeshInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>("FloorMesh");
	FloorMeshInstances->SetupAttachment(SceneComponent);
	
	WallMeshInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>("WallMesh");
	WallMeshInstances->SetupAttachment(SceneComponent);
	
	FoodChildActorComponent = CreateDefaultSubobject<UChildActorComponent>("FoodChildActorComponent");
	FoodChildActorComponent->SetChildActorClass(AFood::StaticClass());
	FoodChildActorComponent->SetupAttachment(SceneComponent);
	
	GridSize = 5;
	TileSize = 100;
	TileScale = 0.1f;
	TileZOffset = -50.f;
	WallScale = 2.f;
}

void APlayArea::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (HasAuthority() && GetWorld() && GetWorld()->GetAuthGameMode<ASnakeGameMode>())
	{
		GridSize = GetWorld()->GetAuthGameMode<ASnakeGameMode>()->GetMapSize();
		SetPlayerSpawns();
	}
}

void APlayArea::OnFoodDestroyed(AActor* Food)
{
	if (IsValid(Food))
	{
		Food->OnDestroyed.RemoveDynamic(this, &APlayArea::OnFoodDestroyed);
	}
	
	SpawnFood();
}

void APlayArea::RegenMap()
{
	GridSize = GetWorld()->GetAuthGameMode<ASnakeGameMode>()->GetMapSize();
	OnRep_GridSize();
	
	const FVector ClientOffset = FVector(static_cast<float>(TileSize) * 2.0f, (GridSize - 1) * TileSize - 2.0f * static_cast<float>(TileSize), 0.0f);
	ClientStart->SetActorLocation(this->GetActorLocation() + ClientOffset);
	
	// Destroying it respawns another one, if one has been spawned
	FoodChildActorComponent->DestroyChildActor();
}

// Called when the game starts or when spawned
void APlayArea::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		RegenMap();
		// SetPlayerSpawns();
		SpawnFood();
		GetWorld()->GetGameState<ASnakeGameState>()->OnGameStageChanged.AddDynamic(this, &APlayArea::OnGameStageChanged);
	}
}

void APlayArea::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (HasAuthority())
	{
		GetWorld()->GetGameState<ASnakeGameState>()->OnGameStageChanged.RemoveDynamic(this, &APlayArea::OnGameStageChanged);
		AFood* FoodActor = Cast<AFood>(FoodChildActorComponent->GetChildActor());
		if (IsValid(FoodActor))
		{
			FoodActor->OnDestroyed.RemoveDynamic(this, &APlayArea::OnFoodDestroyed);
		}
	}
}

void APlayArea::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APlayArea, GridSize);
}

void APlayArea::SetPlayerSpawns()
{
	const ASnakeGameMode* Gm = Cast<ASnakeGameMode>(GetWorld()->GetAuthGameMode());
	
	APlayerStart* HostStart = Cast<APlayerStart>(GetWorld()->SpawnActor(APlayerStart::StaticClass()));
	HostStart->GetCapsuleComponent()->SetMobility(EComponentMobility::Movable);
	HostStart->PlayerStartTag = Gm->HostSpawnName;
	const FVector HostOffset = FVector(static_cast<float>(TileSize) * 2.0f, static_cast<float>(TileSize) * 2.0f, 0.0f);
	HostStart->SetActorLocation(this->GetActorLocation() + HostOffset);
	
	ClientStart = Cast<APlayerStart>(GetWorld()->SpawnActor(APlayerStart::StaticClass()));
	ClientStart->GetCapsuleComponent()->SetMobility(EComponentMobility::Movable);
	ClientStart->PlayerStartTag = Gm->ClientSpawnName;
	const FVector ClientOffset = FVector(static_cast<float>(TileSize) * 2.0f, (GridSize - 1) * TileSize - 2.0f * static_cast<float>(TileSize), 0.0f);
	ClientStart->SetActorLocation(this->GetActorLocation() + ClientOffset);
}

void APlayArea::OnGameStageChanged(int NewGameStage)
{
	RegenMap();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void APlayArea::OnRep_GridSize()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("GridSize replicated: %d"), GridSize));
	WallMeshInstances->ClearInstances();
	FloorMeshInstances->ClearInstances();
	SpawnTiles();
}

void APlayArea::SpawnTiles() const
{
	if (!IsValid(FloorMeshInstances))
	{
		UE_LOG(LogTemp, Error, TEXT("Floor mesh is invalid"));
		return;
	}

	const FVector BaseWorldPos = SceneComponent->GetComponentLocation();
	for (int x = 0; x < GridSize; x++)
	{
		for (int y = 0; y < GridSize; y++)
		{
			const bool bIsHorizontalWall = (x == 0 || x == GridSize-1);
			const bool bIsVerticalWall = (y == 0 || y == GridSize-1);
			if (bIsHorizontalWall || bIsVerticalWall)
			{
				const FVector WallPosition = BaseWorldPos + FVector(x * TileSize, y * TileSize, 0.f);
				FTransform WallTransform = FTransform(WallPosition);
				const FVector WallScaleVector = FVector(WallScale, WallScale, WallScale);
				WallTransform.SetScale3D(WallScaleVector);
				WallMeshInstances->AddInstance(WallTransform, true);
			}
			else
			{
				const FVector TilePosition = BaseWorldPos + FVector(x * TileSize, y * TileSize, TileZOffset);
				FTransform TileTransform = FTransform(TilePosition);
				const FVector TileScaleVector = FVector(TileScale, TileScale, TileScale);
				TileTransform.SetScale3D(TileScaleVector);
				FloorMeshInstances->AddInstance(TileTransform, true);
			}
		}
	}
}

FVector APlayArea::GetRandomFreeFloorLocation() const
{
	float constexpr UpDistance = 50.0f;
	FHitResult HitResult;
	FTransform RandomFloorTransform;
	int constexpr MaxAttempts = 500;
	int CurrentAttempt = 0;
	
	while (CurrentAttempt < MaxAttempts)
	{
		const bool bValidTile = FloorMeshInstances->GetInstanceTransform(
		FMath::RandRange(0, FloorMeshInstances->GetInstanceCount() - 1),
		RandomFloorTransform, true);
		
		const FVector HalfSize = FVector::OneVector * (static_cast<float>(TileSize)/2.0f);
		const bool bUnoccupiedTile = !UKismetSystemLibrary::BoxTraceSingleByProfile(
			GetWorld(),
			RandomFloorTransform.GetLocation() + FVector::UpVector * (HalfSize + 1.0f),
			RandomFloorTransform.GetLocation() + FVector::UpVector * UpDistance,
			HalfSize,
			FRotator::ZeroRotator,
			FName("BlockAll"),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::None,
			HitResult,
			true);
		
		if (!bUnoccupiedTile)
		{
			UE_LOG(LogTemp, Log, TEXT("HIT: %s"), *HitResult.GetActor()->GetName());
		}
		
		if (bValidTile && bUnoccupiedTile)
		{
			break;
		}
		
		CurrentAttempt++;
	}
	
	return RandomFloorTransform.GetLocation();
}

void APlayArea::SpawnFood() const
{
	FVector RandomFloorLocation = GetRandomFreeFloorLocation();
	
	FoodChildActorComponent->CreateChildActor();
	AActor* ChildActor = FoodChildActorComponent->GetChildActor();
	if (!IsValid(ChildActor))
	{
		return;
	}
	
	RandomFloorLocation.Z += TileZOffset * -1;
	// ChildActor->SetActorTransform(FTransform(RandomFloorLocation));
	FoodChildActorComponent->SetWorldLocation(RandomFloorLocation);
	AFood* FoodActor = Cast<AFood>(ChildActor);
	if (FoodActor)
	{
		FoodActor->OnDestroyed.AddDynamic(this, &APlayArea::OnFoodDestroyed);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayArea::SpawnFood() - FoodActor is null, ChildActorComponent prob has wrong class"));
	}
}

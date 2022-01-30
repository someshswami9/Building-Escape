// Fill out your copyright notice in the Description page of Project Settings.
#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFrameWork/Actor.h"

#define OUT
// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	InitialAngle = GetOwner()->GetActorRotation().Yaw;
	CurrentAngle = InitialAngle;
	OpenAngle += InitialAngle ;
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Has Opendorr But Not Have any PressurePlaate"), *GetOwner()->GetName());
	}

	SetAudioComponent();
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetTotalMass()>MassRequiredToOpenDoor) 
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (GetWorld()->GetTimeSeconds()- DoorLastOpened > DoorClosed)
		{
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	CurrentAngle = FMath::Lerp(CurrentAngle, OpenAngle, DeltaTime*DoorOpenSpeed);
	FRotator OpenDoor = GetOwner()->GetActorRotation();
	OpenDoor.Yaw = CurrentAngle;
	GetOwner()->SetActorRotation(OpenDoor);

	CloseDoorAudio = false;
	if (!AudioComponent) { return; }
	if (!OpenDoorAudio) {
		AudioComponent->Play();
		OpenDoorAudio = true;
	}
	
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentAngle = FMath::Lerp(CurrentAngle, InitialAngle, DeltaTime*DoorCloseSpeed);
	FRotator CloseDoor = GetOwner()->GetActorRotation();
	CloseDoor.Yaw = CurrentAngle;
	GetOwner()->SetActorRotation(CloseDoor);

	OpenDoorAudio = false;
	if (!AudioComponent) { return; }
	if (!CloseDoorAudio)
	{
		AudioComponent->Play();
		CloseDoorAudio = true;
	}
}

float UOpenDoor::GetTotalMass() const
{
	float TotalMass = 0.f;
	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);
	for (AActor* Actor : OverlappingActors )
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	return TotalMass;
}

void UOpenDoor::SetAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if(!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s not have any AudioComponent"), *GetOwner()->GetName());
	}
}

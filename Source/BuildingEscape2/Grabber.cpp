// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

#define OUT
// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	CheckPhysicsHandle();

	SetInputComponent();
}

void UGrabber::CheckPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("No PhysicsHandle Found on %s"), *GetOwner()->GetName());
	}
	
}

void UGrabber::SetInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed,this,&UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

void UGrabber::Grab()
{
	FHitResult  GrabHitObject = HitObject();
	UPrimitiveComponent* PrimitiveComponent = GrabHitObject.GetComponent();
	if (!PhysicsHandle) { return; }
	if (GrabHitObject.GetActor())
	{
		PhysicsHandle->GrabComponentAtLocation(PrimitiveComponent, NAME_None, GetPlayersReach());
	}
}

void UGrabber::Release()
{
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetPlayersReach());
	}

	
}

 FHitResult UGrabber::HitObject() const
{
	FHitResult Hit;
	FCollisionQueryParams TakeParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(OUT Hit,GetPlayersPosInWorld(), GetPlayersReach(), FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TakeParams);
	return Hit;
}

 FVector UGrabber::GetPlayersReach() const
 {
	 FVector PlayerViewLocation;
	 FRotator PlayerViewRoation;

	 GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewLocation, OUT PlayerViewRoation);

	 return PlayerViewLocation + PlayerViewRoation.Vector()*Reach;
 }

 FVector UGrabber::GetPlayersPosInWorld() const
 {
	 FVector PlayerViewLocation;
	 FRotator PlayerViewRoation;

	 GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewLocation, OUT PlayerViewRoation);

	 return PlayerViewLocation;
 }


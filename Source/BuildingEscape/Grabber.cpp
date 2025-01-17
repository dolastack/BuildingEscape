// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	//FString ObjectLocation = GetOwner()->GetTransform().GetLocation().ToString();
    //UE_LOG(LogTemp, Warning, TEXT("Grabber reporting for duty %s"), *ObjectLocation);

	///look for attached physics handle
	FindPhysicsHandleComponent();
	SetupInputComponent();
	
	
	
}

void UGrabber::FindPhysicsHandleComponent() {
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr) {

		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component "), *GetOwner()->GetName());
	}
}

void UGrabber::SetupInputComponent() {
	///look for attached input handle
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		UE_LOG(LogTemp, Warning, TEXT("input component "));
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);

	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s missing input component "), *GetOwner()->GetName());
	}
	// ...
}

void UGrabber::Grab() {

	UE_LOG(LogTemp, Warning, TEXT("Grab pressed"), *GetOwner()->GetName());
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	if (ActorHit != nullptr) {

		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Got nothing"), *GetOwner()->GetName());
	}

}

void UGrabber::Release() {

	UE_LOG(LogTemp, Warning, TEXT("Grab released"), *GetOwner()->GetName());
	PhysicsHandle->ReleaseComponent();

}

 FHitResult UGrabber::GetFirstPhysicsBodyInReach() const {
	 FVector PlayerViewPointLocation;
	 FRotator PlayerViewPointRotation;
	 GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		 OUT PlayerViewPointLocation,
		 OUT PlayerViewPointRotation
	 );


	 FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
	 
	 
	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(255, 0, 0),
		false,
		0.f,
		0.f,
		10

	); 

	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	AActor* ActorHit = Hit.GetActor();
	if (ActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *ActorHit->GetName());
	}
	return Hit;

}

 FVector UGrabber::GetReachLineEnd() {
	 FVector PlayerViewPointLocation;
	 FRotator PlayerViewPointRotation;

	 GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		 OUT PlayerViewPointLocation,
		 OUT PlayerViewPointRotation
	 );

	 FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
	 return LineTraceEnd;
 }

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//GetFirstPhysicsBodyInReach();
	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(GetReachLineEnd());
	}

}


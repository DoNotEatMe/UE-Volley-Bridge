// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIPlayer.h"
#include "GameFramework/Actor.h"
#include "Ball.generated.h"

UCLASS()
class RIVE_API ABall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	UStaticMeshComponent* Sphere;

	UPROPERTY()
	FVector SpherePos;

	UPROPERTY()
	class AAIPlayer* Sender;

	UPROPERTY()
	AAIPlayer* Receiver;

	UPROPERTY()
	FVector AIStartPos;

	UPROPERTY()
	FVector TargetPos;

	UPROPERTY()
	class AVolleyGameModeCPP* GameMode;

	UFUNCTION()
	void BallServe(AAIPlayer* SenderF, AAIPlayer* ReceiverF);

	// ball serve phys

	UPROPERTY()
	float PeakHeight;

	UPROPERTY()
	float NetHeight = 250.f;

	UPROPERTY()
	float HeightDifferenceAscent;
	 
	UPROPERTY()
	float HeightDifferenceDescent;

	UPROPERTY()
	float TimeToPeak;

	UPROPERTY()
	float TimeToDescend;

	UPROPERTY()
	float Gravity = 980.f;

	UPROPERTY()
	float TotalTimeOfFlight;

	// Hit 
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY()
	AAIPlayer* LastHitActor;
	
};

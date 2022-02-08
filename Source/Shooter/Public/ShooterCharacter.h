// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"


UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//Creation By Developer


private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* MyCamera;

	//Base Turn Rate in deg/sec, Other Scalling may affect Final turn Rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate; // Turn sides

	//BaseLookup / down rate, in degrees by sec, other scalling can affect final turn rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate; //Look Up with camera

public:

	//This is a getter for the cameraBoom
	//It is constant Because we are not planning to change any members of this class
	//Returns CameraBoom SubjObject
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }



	FORCEINLINE UCameraComponent* GetCamera() const { return MyCamera; }
	
protected:


	void MoveForward(float Value); //Call Foward /Backwards Input
	void MoveRight(float Value);   //Call Right/Left Input

	// Called via input to turn at a given rate // Look up a given rate
	//@Param Rate this is a normalized rate,  i.e  1.0 means 100% of desired rate
	void TurnAtRate(float Rate);

	void LookUpRate(float Rate);

};

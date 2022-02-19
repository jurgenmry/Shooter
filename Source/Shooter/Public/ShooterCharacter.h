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


	//========================= Base Turn Up Side For Character ======================================//

	//Base Turn Rate in deg/sec, Other Scalling may affect Final turn Rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate; // Turn sides

	//BaseLookup / down rate, in degrees by sec, other scalling can affect final turn rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate; //Look Up with camera




	//=========================     Controller Turn Rates      ======================================//

	//Turn Rate While not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float HipTurnRate; //Hip Firing

	//HipLookUpRate Rate While not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	//Turn Rate While aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;

	//HipLookUpRate Rate While Aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate;



	//=========================     Mouse Turn Rates      ======================================//

	//Scale Factor For Mouse Look Sensitivity. Turn Rate when not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;

	//Scale Factor For Mouse Look Sensitivity. Look up Rate when not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	//Scale Factor For Mouse Look Sensitivity. Turn Rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;

	//Scale Factor For Mouse Look Sensitivity.  Look up when  aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;



	//==================================================================================================//

	//Randomize gunshot sound cue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	//Flash spawn at barrelSocket
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	//Particles spwaned after bullet impact
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;


	//Smoke trail for bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	// True WHne aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Combat", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	//Defaul camera defaul view value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CameraDefaulFOV;

	//Field of view value when zoomed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV;

	//CurrentField of view this frame (for interpolation)
	float CameraCurrentFOV;
	
	// Interpolation of the speed when zooming when aiming
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float ZoomInterpolationSpeed; 

public:

	//This is a getter for the cameraBoom
	//It is constant Because we are not planning to change any members of this class
	//Returns CameraBoom SubjObject
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }


	FORCEINLINE UCameraComponent* GetMyCamera() const { return MyCamera; }

	FORCEINLINE bool GetAiming() const { return bAiming; }
	
protected:


	void MoveForward(float Value); //Call Foward /Backwards Input
	void MoveRight(float Value);   //Call Right/Left Input

	// Called via input to turn at a given rate // Look up a given rate
	//@Param Rate this is a normalized rate,  i.e  1.0 means 100% of desired rate
	void TurnAtRate(float Rate);

	void LookUpRate(float Rate);


	//Rotate Controller base on mouse Y and Y movement
	void Turn(float Rate);
	void LookUp(float Rate);


	//call when the fire button is pressed
	void FireWeapon(); 

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	
	//Set the aiming to true or false when a button is pressed //
	void AimingButtonPressed();
	void AimingButtonReleased();
	
	//This funtion returns the aim of camera with interpolation
	void AimingCameraWithInter(float DeltaTime); 

	//Set Base turn Rate and base look up rate on aiming 
	void SetLookRates();
};

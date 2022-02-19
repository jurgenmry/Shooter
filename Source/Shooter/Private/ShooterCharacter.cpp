// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/AnimMontage.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"


// Sets default values
AShooterCharacter::AShooterCharacter()
	: BaseTurnRate (45.0f)
	, BaseLookUpRate (45.0f)

	//Turn Rates for Aiming Not Aiming
	, HipTurnRate(90.0f)
	, HipLookUpRate(90.0f)
	, AimingTurnRate(20.0f)
	, AimingLookUpRate(20.0f)

	//Camara Field Ov view Values for controller
	, bAiming(false)
	, CameraDefaulFOV(0.0f)  //Setting this on Beging play
	, CameraZoomedFOV(35.0f)
	, ZoomInterpolationSpeed(20.0f)
	, CameraCurrentFOV (0.0f)

	//Mouse Look sensitivity scale factors
	, MouseHipTurnRate(1.0f)
	, MouseHipLookUpRate(1.0f)
	, MouseAimingTurnRate(0.2f)
	, MouseAimingLookUpRate(0.2f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Creation Of Camera Boom (Pulls in towards the character in case of collision)
	CameraBoom = CreateDefaultSubobject <USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.0f;			//Define the distance with the Root component
	CameraBoom->bUsePawnControlRotation = true;		//Use the rotation of the controllet for the Boom
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 70.0f);


	//Create A Follow Camera

	MyCamera = CreateDefaultSubobject <UCameraComponent>(TEXT("MyCamera"));
	MyCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attacht the camera at socket
	MyCamera->bUsePawnControlRotation = true; //camera does not rotate relative to arm
	

	//Dont allow the character to rotate when the controller does it 
	//Let the controller only affect the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;


	//Allow the character to move only with the direction
	//Confirgure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in direction of movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.0f;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (MyCamera)
	{
		CameraDefaulFOV = GetMyCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaulFOV;  
	}

}


// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handle interpolation for zoom when aiming
	AimingCameraWithInter(DeltaTime);
	
	//Change look Sensitivity based on aiming
	SetLookRates();

}



void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller)&& Value != 0)
	{
		//Find Out wich way if forward
		const FRotator Rotation = Controller->GetControlRotation();     // Gets the Rotations of the controller
		const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);// Direction of the yaw

		const FVector Direction = FRotationMatrix{ YawRotation }.GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller) && Value != 0)
	{
		//Find Out wich way if Right
		const FRotator Rotation = Controller->GetControlRotation();     // Gets the Rotations of the controller
		const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);// Direction of the yaw

		const FVector Direction = FRotationMatrix{ YawRotation }.GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Turn(float Rate)
{
	float TurnScaleFactor{};
	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;

	}

	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}

	AddControllerYawInput(Rate * TurnScaleFactor);
}

void AShooterCharacter::LookUp(float Rate)
{

	float LookUpScaleFactor{};
	if (bAiming)
	{
		LookUpScaleFactor = MouseAimingLookUpRate;

	}

	else
	{
		LookUpScaleFactor = MouseHipLookUpRate;
	}

	AddControllerPitchInput(Rate * LookUpScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
	//UE_LOG(LogTemp, Warning, TEXT("Fire Weapon"));
	
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	} 

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}


		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(),
		BeamEnd);

		if (bBeamEnd)
		{
			//Spawn Impact Particle after updating beam end point
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);

			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}

	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	//Get Currense Size of the view Port
	FVector2D ViewportSize;

	//Get Screen Space Location of Locations
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//GHet World Position and direction of the crosshairs
	FVector2D CrosshairLocation{ ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f };
	CrosshairLocation.Y -= 50.0f;
	FVector CrosshaitWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation,
		CrosshaitWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		FHitResult ScreenTraceHit;
		const FVector Start = CrosshaitWorldPosition;
		const FVector End = CrosshaitWorldPosition + CrosshairWorldDirection * 50'000.0f;

		//Set beam end point to line trace end point
		OutBeamLocation = End;

		//Trace outward From Crosshairs world location
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECollisionChannel::ECC_Visibility);

		if (ScreenTraceHit.bBlockingHit) // was there a trace hit   ///////
		{
			//beam end point if now trace hit location
			OutBeamLocation = ScreenTraceHit.Location;

			//Perform a second trace, this time from the gun Barrel. This helps us to avoid bullet overlap object in middle

			FHitResult WeaponTracehit;
			const FVector WeaponTraceStart = MuzzleSocketLocation;
			const FVector WeaponTraceEnd = OutBeamLocation;
			GetWorld()->LineTraceSingleByChannel(WeaponTracehit, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

			if (WeaponTracehit.bBlockingHit) //Object Between barre and beam end point
			{
				OutBeamLocation = WeaponTracehit.Location;
			}

			return true;
		}
	}

	return false;
}

void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;	
}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false; 
}

void AShooterCharacter::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}

	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AShooterCharacter::AimingCameraWithInter(float DeltaTime)
{

	//Aiming button press. Set Currennt camara field of view
	if (bAiming)
	{
		//Interpolate to Zoom
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraZoomedFOV,
			DeltaTime,
			ZoomInterpolationSpeed);
	}
	else
	{
		//Interpolate to default
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraDefaulFOV,
			DeltaTime,
			ZoomInterpolationSpeed);
	}

	GetMyCamera()->SetFieldOfView(CameraCurrentFOV);
}



// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);

	//For Aiming the weapon
	PlayerInputComponent->BindAction("AimButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

	//For controller u other Key
	PlayerInputComponent->BindAxis("TurnRightRate", this, &AShooterCharacter::TurnAtRate); 
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpRate);

	//For the Mouse Movement
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//For the firing of weapons
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);


}
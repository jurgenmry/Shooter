// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ShooterAnimInstance.h"


void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter> (TryGetPawnOwner());
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		//Get the lateral speed of character from velocity
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size(); // Does not take speed on the z axis

		// is the character in the air?
		bIsInTheAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		//Is the character Accelarating ?
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f)
		{
			bIsAccelerating = true;
		}

		else
		{
			bIsAccelerating = false;
		}

		//What is the rotation corresponding to the direction we are aim at
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();

		FString RotationMessage = FString::Printf(TEXT("Base Aim Rotation: %f"), AimRotation.Yaw);
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter -> GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		

		if (ShooterCharacter->GetVelocity().Size() > 0.0f)
		{
			LastMovementOfsetYaw = MovementOffsetYaw;
		}

		bAiming = ShooterCharacter->GetAiming();
		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Red, RotationMessage);
		}
		*/

	}
}

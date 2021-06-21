// Copyright Saber Studio. All Rights Reserved.


#include "ShooterGameSession.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

void AShooterGameSession::RegisterServer()
{
	Super::RegisterServer();

	UE_LOG(LogTemp, Log, TEXT("Registering server and creating session --------"));
	IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();
	if (OnlineSubSystem == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No online subsystem found!!"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("Got online subsystem"));
	IOnlineSessionPtr Session = OnlineSubSystem->GetSessionInterface();
	UE_LOG(LogTemp, Log, TEXT("Got session interface"));
	FOnlineSessionSettings Settings;

	Settings.bIsDedicated = true;
	Settings.bIsLANMatch = false;
	Settings.bAllowJoinInProgress = true;
	Settings.bShouldAdvertise = true;
	Settings.NumPublicConnections = 64;
	Settings.bUsesPresence = true;

	UE_LOG(LogTemp, Log, TEXT("Initialized settings"));

	Session->CreateSession(0, GameSessionName, Settings);

	UE_LOG(LogTemp, Log, TEXT("Session created --------")); 
}

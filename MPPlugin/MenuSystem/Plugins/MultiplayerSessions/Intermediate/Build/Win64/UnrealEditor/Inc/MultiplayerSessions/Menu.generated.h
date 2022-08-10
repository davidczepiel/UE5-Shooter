// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef MULTIPLAYERSESSIONS_Menu_generated_h
#error "Menu.generated.h already included, missing '#pragma once' in Menu.h"
#endif
#define MULTIPLAYERSESSIONS_Menu_generated_h

#define FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_SPARSE_DATA
#define FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execJoinButtonClick); \
	DECLARE_FUNCTION(execHostButtonClick); \
	DECLARE_FUNCTION(execOnStartsession); \
	DECLARE_FUNCTION(execOnDestroySession); \
	DECLARE_FUNCTION(execOnCreateSession); \
	DECLARE_FUNCTION(execMenuSetUp);


#define FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execJoinButtonClick); \
	DECLARE_FUNCTION(execHostButtonClick); \
	DECLARE_FUNCTION(execOnStartsession); \
	DECLARE_FUNCTION(execOnDestroySession); \
	DECLARE_FUNCTION(execOnCreateSession); \
	DECLARE_FUNCTION(execMenuSetUp);


#define FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUMenu(); \
	friend struct Z_Construct_UClass_UMenu_Statics; \
public: \
	DECLARE_CLASS(UMenu, UUserWidget, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/MultiplayerSessions"), NO_API) \
	DECLARE_SERIALIZER(UMenu)


#define FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_INCLASS \
private: \
	static void StaticRegisterNativesUMenu(); \
	friend struct Z_Construct_UClass_UMenu_Statics; \
public: \
	DECLARE_CLASS(UMenu, UUserWidget, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/MultiplayerSessions"), NO_API) \
	DECLARE_SERIALIZER(UMenu)


#define FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UMenu(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UMenu) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UMenu); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UMenu); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UMenu(UMenu&&); \
	NO_API UMenu(const UMenu&); \
public:


#define FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UMenu(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UMenu(UMenu&&); \
	NO_API UMenu(const UMenu&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UMenu); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UMenu); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UMenu)


#define FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_10_PROLOG
#define FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_SPARSE_DATA \
	FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_RPC_WRAPPERS \
	FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_INCLASS \
	FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_SPARSE_DATA \
	FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_INCLASS_NO_PURE_DECLS \
	FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_13_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> MULTIPLAYERSESSIONS_API UClass* StaticClass<class UMenu>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_MenuSystem_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS

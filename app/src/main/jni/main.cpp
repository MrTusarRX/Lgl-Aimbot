
//Coded By MrTusarRX


#include <list>
#include <vector>
#include <string>
#include <string.h>
#include <pthread.h>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "Unity/Vector2.h"
#include "Unity/Vector3.h"
#include "Unity/Rect.h"
#include "Unity/Color.h"
#include "Unity/Quaternion.h"
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "Includes/Chams.h"
#include "Includes/MonoString.h"
#include "Includes/Strings.h"
#include "KittyMemory/MemoryPatch.h"
#include "menu.h"
#include "NepDraw.h"
#include "hooks.h"
#include "ESPManager.h"
ESPManager *espManager;
NepEsp es;

#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

bool ESP, ESPLine, EspPlayer, ESPLineTest, ESPShowSpeed, ESPHealth,telekill;
std::string PlayerEsp = "Player: ";
std::string ShowSpeed = "Speed Player: ";
std::string ShowHealth = "Health: ";
std::string strtest = "Balo";
Color color = Color::Yellow();

void (*set_rotation)(void *player, Quaternion rotation);

void (*SetLocalScale)(void *transform, Vector3);
bool AIMBOT;
Quaternion LookRotation;
Quaternion PlayerLook;

Vector3 add(Vector3 a, Vector3 b) {
    Vector3 c;
    c.x= a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    return c;
}

void DrawESP(NepEsp esp, int screenWidth, int screenHeight) {
    if (EspPlayer) {
        std::string playerText = "Player: " + std::to_string(espManager->enemies->size());
        esp.DrawText(color, playerText.c_str(), Vector2(screenWidth / 2 + 60, screenHeight / 1.2), 25);
    }

    if (ESPLineTest) {
        esp.DrawLine(color, 2, Vector2(screenWidth / 2, 0), Vector2(screenWidth / 2, screenHeight));
        esp.DrawLine(color, 2, Vector2(0, screenHeight / 2), Vector2(screenWidth, screenHeight / 2));
    }

    if (espManager->enemies->empty()) {
        return;
    }

    for (int i = 0; i < espManager->enemies->size(); ++i) {
        void* Player = (*espManager->enemies)[i]->object;

        if (!PlayerAlive(Player)) {
            espManager->removeEnemyGivenObject(Player);
            continue;
        }

        float health = GetPlayerHealth(Player);
        Vector3 playerPos = GetPlayerLocation(Player);
        Vector3 screenPos = WorldToScreenPoint(get_camera(), playerPos);

        Vector2 drawFrom = Vector2(screenWidth / 2, 0);
        Vector2 drawTo = Vector2(screenPos.x + 5, screenHeight - screenPos.y - 10.0f);

        if (ESPLine) {
            esp.DrawLine(color, 2, drawFrom, drawTo);
        }

        if (ESPHealth) {
            std::string healthText = "Health: " + std::to_string(health);
            esp.DrawText(color, healthText.c_str(), drawTo, 20);
        }
    }
}

    
extern "C"
JNIEXPORT void JNICALL
Java_uk_lgl_modmenu_FloatingModMenuService_DrawOn(JNIEnv *env, jclass type, jobject espView, jobject canvas) {
                es = NepEsp(env, espView, canvas);
    if (es.isValid()){
        DrawESP(es, es.getWidth(), es.getHeight());
    }
    
 }

void *get_myPlayer(void *player) {
    myPlayer = player;
    return myPlayer;
}

void (* old_myupdate)(void *Player);
void myupdate(void *Player) {
	if(Player != NULL){
		 
	}
	get_myPlayer(Player);
	return old_myupdate(Player);
}
void *enemyPlayer = NULL;
void *playerlate = NULL;

void *get_Player(void *player) {
    playerlate = player;
    return playerlate;
}

void* closestEnemy = nullptr;
float closestDistance = FLT_MAX;

void (*update)(void *player);

void _update(void *player) {
    if (player != NULL) {
        void* camera = get_camera();

        Vector3 PlayerLocation = get_position(getTransform(myPlayer));
        Vector3 enemyLocation = get_position(getTransform(player));
        Vector3 CorrectPlayerPos = WorldToScreenPoint(camera, PlayerLocation);
        Vector3 CorrectEnemyPos = WorldToScreenPoint(camera, enemyLocation);
//closest enemy chechker
        float distance = Vector3::Distance(CorrectPlayerPos, CorrectEnemyPos);
        if (distance < closestDistance) {
            closestDistance = distance;
            closestEnemy = player;
            get_Player(player); 
        }

        if (ESP) {
            espManager->tryAddEnemy(player);
        }
    }
//shit aimbot
  
    if (AIMBOT && player == playerlate && playerlate != NULL) {
        void* camera = get_camera();
        Vector3 PlayerLocation = get_position(getTransform(myPlayer));
        Vector3 enemyLocation = get_position(getTransform(playerlate));
        Vector3 Head = Vector3(enemyLocation.x, enemyLocation.y + 0.5f, enemyLocation.z);

        Quaternion PlayerLook = Quaternion::LookRotation(Head - PlayerLocation, Vector3(0, 1, 0));
        set_rotation(getTransform(myPlayer), PlayerLook);
        set_rotation(getTransform(camera), PlayerLook);
    }

    return update(player);
}

void (*old_Player_ondestroy)(void *player);
void Player_ondestroy(void *player) {
    if (player != NULL) {
        old_Player_ondestroy(player);
        espManager->removeEnemyGivenObject(player);
    }
}   

void *hack_thread(void *) {
    ProcMap il2cppMap;
    do {
        il2cppMap = KittyMemory::getLibraryMap("libil2cpp.so");
        sleep(1);
    } while (!isLibraryLoaded("libil2cpp.so") && mlovinit());
    espManager = new ESPManager();

    do {
        sleep(1);
    } while (!isLibraryLoaded (OBFUSCATE("libMyLibName.so")));
   
	
			MSHookFunction((void *) getAbsoluteAddress("libil2cpp.so", 0x2AD358), (void *) &myupdate, (void **) &old_myupdate); //my update 
	
		MSHookFunction((void *) getAbsoluteAddress("libil2cpp.so", 0x294160), (void *) &_update, (void **) &update);//enemy update
		MSHookFunction((void *) getAbsoluteAddress("libil2cpp.so", 0x291FB0), (void *) &Player_ondestroy, (void **) &old_Player_ondestroy);//enemy destroy 
		set_rotation = (void (*)(void*, Quaternion))getAbsoluteAddress("libil2cpp.so", 0x72FBFC); //set_rotation_Injected
	
		
	return NULL;
}

extern "C" {
    JNIEXPORT jobjectArray
    JNICALL
    Java_uk_lgl_modmenu_FloatingModMenuService_getFeatureList(JNIEnv *env, jobject context) {
        jobjectArray ret;
        const char *features[] = {
            OBFUSCATE("0_Toggle_Enable Esp"),//0
            OBFUSCATE("1_Toggle_ESP Line"),//1
			OBFUSCATE("2_Toggle_ESP Line Test"),//1
            OBFUSCATE("3_Toggle_ESP Player"),
			OBFUSCATE("4_SeekBar_OffSet_0_4"),
			OBFUSCATE("5_Toggle_ESP Speed"),
			OBFUSCATE("6_Toggle_Aimbot"),
			OBFUSCATE("7_Toggle_ESP Health"),

        };
        int Total_Feature = (sizeof features / sizeof features[0]);
        ret = (jobjectArray)
              env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                  env->NewStringUTF(""));

        for (int i = 0; i < Total_Feature; i++)
            env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

        return (ret);
    }


    JNIEXPORT void JNICALL
    Java_uk_lgl_modmenu_Preferences_Changes(JNIEnv *env, jclass clazz, jobject obj,
                                            jint featNum, jstring featName, jint value,
                                            jboolean boolean, jstring str) {

        switch (featNum) {
        case 0:
            ESP = boolean;
            break;
        case 1:
            ESPLine = boolean;
            break;
		case 2:
            ESPLineTest = boolean;
            break;
        case 3:
			EspPlayer = boolean;
			break;
		case 4:
			if(value == 0) color = Color::Yellow();
			if(value == 1) color = Color::White();
			if(value == 2) color = Color::Black();
			if(value == 3) color = Color::Red();
			if(value == 4) color = Color::Blue();
			break;
		case 5:
			ESPShowSpeed = boolean;
			break;
		case 6:
			AIMBOT = boolean;
			break;
		case 7:
			ESPHealth = boolean;
			break;
        }
    }
}

__attribute__((constructor))
void lib_main() {

    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}


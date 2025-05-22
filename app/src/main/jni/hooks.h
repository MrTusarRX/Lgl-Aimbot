

void *getTransform(void *player) {
    if (!player) return NULL;
    static const auto get_transform_injected = reinterpret_cast<uint32_t(__fastcall *)(void *)>(getAbsoluteAddress("libil2cpp.so", 0x3F8A54));
    return (void *) get_transform_injected(player);
}
Vector3 get_position(void *transform) {
    if (!transform)return Vector3();
    Vector3 position;
    static const auto get_position_injected = reinterpret_cast<uint32_t(__fastcall *)(void *,Vector3 &)>(getAbsoluteAddress("libil2cpp.so", 0x72F790));
    get_position_injected(transform, position);
    return position;
}


Vector3 WorldToScreenPoint(void *transform, Vector3 test) {
    if (!transform)return Vector3();
    Vector3 position;
    static const auto WorldToScreenPoint_Injected = reinterpret_cast<uint32_t(__fastcall *)(void *,Vector3, int, Vector3 &)>(getAbsoluteAddress("libil2cpp.so", 0x3F66F0));
      WorldToScreenPoint_Injected(transform, test, 4, position);
      return position;
}

void *get_camera() {
    static const auto get_camera_injected = reinterpret_cast<uint32_t(__fastcall *)()>(getAbsoluteAddress("libil2cpp.so", 0x3F6AF4));
    return (void *) get_camera_injected();
}

void set_position(void* transform, Vector3 test) {
    if (transform) {
        static const auto set_position_injected = reinterpret_cast<uintptr_t(__fastcall*)(void*, Vector3)>(getAbsoluteAddress("libil2cpp.so", 0x72F848));
        set_position_injected(transform, test);
        }

 }





Vector3 GetPlayerLocation(void *player) {
    return get_position(getTransform(player));
}


int GetPlayerHealth(void *player) {
    return *(float *) ((uintptr_t) player + 0xA0); 
}
/*
int GetMaxHealth(void *player) {
    return *(float *) ((uintptr_t) player + 0xA4); 
}

bool GetPlayerDead(void *player) {
    return *(bool *) ((uintptr_t) player + 0xA8);
}
*/

bool PlayerAlive(void *player) {
   bool (*get_IsAlive)(void* players) = (bool(*)(void*))getAbsoluteAddress("libil2cpp.so", 0x717D48);
    return get_IsAlive(player);
}

bool IsPlayerDead(void *player) {
    return PlayerAlive(player) == false;
}
int string_to_int(string str) 
{ 
    int value;
    value = std::stoi(str);
    return value;
}
string float_to_string (int value) 
{
    string str; 
    str = std::to_string(value);
    str += "M";
    return str;
}

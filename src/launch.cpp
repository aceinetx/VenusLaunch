#include <launch.h>

std::string generateJavaCmd(Settings &settings)
{
  std::string args = "";
  args.append("\"" + settings.java_path + "\" ");
  args.append("-noverify ");
  args.append("-XX:+UnlockExperimentalVMOptions -XX:+UseG1GC -XX:G1NewSizePercent=20 -XX:G1ReservePercent=20 -XX:MaxGCPauseMillis=50 -XX:G1HeapRegionSize=32M -XX:+DisableExplicitGC -XX:+AlwaysPreTouch -XX:+ParallelRefProcEnabled ");
  args.append(std::format("-Xms{}M -Xmx{}M ", settings.ram_min, settings.ram_max));
  args.append("-Dfile.encoding=UTF-8 ");
  args.append(std::format("-Dlog4j.configurationFile={}\\assets\\log_configs\\patched-variant-2.7.xml ", settings.game_path));
  args.append("-XX:HeapDumpPath=MojangTricksIntelDriversForPerformance_javaw.exe_minecraft.exe.heapdump ");
  args.append(std::format("-Djava.library.path={}\\VenusFree\\natives ", settings.game_path));
  args.append("-Dminecraft.launcher.brand=java-minecraft-launcher -Dminecraft.launcher.version=1.6.84-j ");
  args.append(replaceAll("-cp %VENUSLAUNCH_GAMEPATH%\\libs\\com\\turikhay\\ca-fixer\\1.0\\ca-fixer-1.0.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\ru\\tlauncher\\patchy\\1.0.0\\patchy-1.0.0.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\oshi-project\\oshi-core\\1.1\\oshi-core-1.1.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\net\\java\\dev\\jna\\jna\\4.4.0\\jna-4.4.0.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\net\\java\\dev\\jna\\platform\\3.4.0\\platform-3.4.0.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\com\\ibm\\icu\\icu4j\\66.1\\icu4j-66.1.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\com\\mojang\\javabridge\\1.0.22\\javabridge-1.0.22.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\net\\sf\\jopt-simple\\jopt-simple\\5.0.3\\jopt-simple-5.0.3.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\io\\netty\\netty-all\\4.1.25.Final\\netty-all-4.1.25.Final.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\com\\google\\guava\\guava\\21.0\\guava-21.0.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\org\\apache\\commons\\commons-lang3\\3.5\\commons-lang3-3.5.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\commons-io\\commons-io\\2.5\\commons-io-2.5.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\commons-codec\\commons-codec\\1.10\\commons-codec-1.10.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\net\\java\\jinput\\jinput\\2.0.5\\jinput-2.0.5.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\net\\java\\jutils\\jutils\\1.0.0\\jutils-1.0.0.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\com\\mojang\\brigadier\\1.0.17\\brigadier-1.0.17.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\com\\mojang\\datafixerupper\\4.0.26\\datafixerupper-4.0.26.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\com\\google\\code\\gson\\gson\\2.8.0\\gson-2.8.0.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\by\\ely\\authlib\\3.11.49.2\\authlib-3.11.49.2.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\org\\apache\\commons\\commons-compress\\1.8.1\\commons-compress-1.8.1.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\org\\apache\\httpcomponents\\httpclient\\4.3.3\\httpclient-4.3.3.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\commons-logging\\commons-logging\\1.1.3\\commons-logging-1.1.3.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\org\\apache\\httpcomponents\\httpcore\\4.3.2\\httpcore-4.3.2.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\it\\unimi\\dsi\\fastutil\\8.2.1\\fastutil-8.2.1.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\org\\apache\\logging\\log4j\\log4j-api\\2.8.1\\log4j-api-2.8.1.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\org\\apache\\logging\\log4j\\log4j-core\\2.8.1\\log4j-core-2.8.1.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\org\\lwjgl\\lwjgl\\3.2.2\\lwjgl-3.2.2.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\org\\lwjgl\\lwjgl-jemalloc\\3.2.2\\lwjgl-jemalloc-3.2.2.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\org\\lwjgl\\lwjgl-openal\\3.2.2\\lwjgl-openal-3.2.2.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\org\\lwjgl\\lwjgl-opengl\\3.2.2\\lwjgl-opengl-3.2.2.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\org\\lwjgl\\lwjgl-glfw\\3.2.2\\lwjgl-glfw-3.2.2.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\org\\lwjgl\\lwjgl-stb\\3.2.2\\lwjgl-stb-3.2.2.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\org\\lwjgl\\lwjgl-tinyfd\\3.2.2\\lwjgl-tinyfd-3.2.2.jar;%VENUSLAUNCH_GAMEPATH%\\libs\\com\\mojang\\text2speech\\1.11.3\\text2speech-1.11.3.jar;%VENUSLAUNCH_GAMEPATH%\\VenusFree\\VenusFree.jar ", "%VENUSLAUNCH_GAMEPATH%", settings.game_path));
  args.append("-Xss2M net.minecraft.client.main.Main ");
  args.append(std::format("--username {} ", settings.username));
  args.append("--version VenusFree ");
  args.append(std::format("--gameDir {} ", settings.game_path));
  args.append(std::format("--assetsDir {}\\assets ", settings.game_path));
  args.append("--assetIndex 1.16 --uuid 0 --accessToken 0 --userType legacy --versionType release ");
  args.append(std::format("--width {} ", settings.window_width));
  args.append(std::format("--height {}", settings.window_height));
  return args;
}
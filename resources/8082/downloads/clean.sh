#calculating the current available storage
Storage=$(df -h "$HOME" | grep "$HOME" | awk '{print($4)}' | tr 'i' 'B')
if [ "$Storage" == "0BB" ];
then
Storage="0B"
fi
echo "\033[33m\n — Available Storage Before Cleaning : || $Storage || —\033[0m"
#df -h | grep Users | awk '{print "Before:\t"$2 " = " $3 " + " $4}'
echo "\033[31m\n — Cleaning ...\n\033[0m "
#42 Caches
/bin/rm -rf ~/Library/*.42* &>/dev/null
/bin/rm -rf ~/*.42* &>/dev/null
/bin/rm -rf ~/.zcompdump* &>/dev/null
/bin/rm -rf ~/.cocoapods.42_cache_bak* &>/dev/null
#Trash
/bin/rm -rf ~/.Trash/* &>/dev/null
#General Cache files
/bin/rm -rf ~/Library/Caches/* &>/dev/null
/bin/rm -rf ~/Library/Application\ Support/Caches/* &>/dev/null
#Slack, VSCode, Discord and Chrome Caches
/bin/rm -rf ~/Library/Application\ Support/Slack/Service\ Worker/CacheStorage/* &>/dev/null
/bin/rm -rf ~/Library/Application\ Support/Code/User/workspaceStorage/* &>/dev/null
/bin/rm -rf ~/Library/Application\ Support/discord/Cache/* &>/dev/null
/bin/rm -rf ~/Library/Application\ Support/discord/Code\ Cache/js* &>/dev/null
/bin/rm -rf ~/Library/Application\ Support/Google/Chrome/Profile\ [0-9]/Service\ Worker/CacheStorage/* &>/dev/null
/bin/rm -rf ~/Library/Application\ Support/Google/Chrome/Default/Service\ Worker/CacheStorage/* &>/dev/null
/bin/rm -rf ~/Library/Application\ Support/Google/Chrome/Profile\ [0-9]/Application\ Cache/* &>/dev/null
/bin/rm -rf ~/Library/Application\ Support/Google/Chrome/Default/Application\ Cache/* &>/dev/null
#.DS_Store files
find ~/Desktop -name .DS_Store -depth -exec /bin/rm {} \; &>/dev/null
#tmp downloaded files with browsers
/bin/rm -rf ~/Library/Application\ Support/Chromium/Default/File\ System &>/dev/null
/bin/rm -rf ~/Library/Application\ Support/Chromium/Profile\ [0-9]/File\ System &>/dev/null
/bin/rm -rf ~/Library/Application\ Support/Google/Chrome/Default/File\ System &>/dev/null
/bin/rm -rf ~/Library/Application\ Support/Google/Chrome/Profile\ [0-9]/File\ System &>/dev/null
#things related to pool (piscine)
/bin/rm -rf ~/Desktop/Piscine\ Rules\ *.mp4
/bin/rm -rf ~/Desktop/PLAY_ME.webloc

rm -rf ~/Library/Application\ Support/Slack/Cache
rm -rf ~/Library/Application\ Support/Slack/Code\ Cache/
rm -rf ~/Library/Application\ Support/Slack/Cache/
rm -rf ~/Library/Application\ Support/Slack/Service\ Worker/CacheStorage/
rm -rf ~/Library/Application\ Support/Spotify/PersistentCache
rm -rf ~/Library/Application\ Support/Code/Cache
rm -rf ~/Library/Application\ Support/Code/Code\ Cache
rm -rf ~/Library/Application\ Support/Code/CachedData/*
rm -rf ~/Library/Application\ Support/Code/CachedData
rm -rf ~/Library/Application\ Support/Code/CachedExtension
rm -rf ~/Library/Application\ Support/Code/CachedExtensions
rm -rf ~/Library/Application\ Support/Code/CachedExtensionVSIXs
rm -rf ~/Library/Application\ Support/Code/User/workspaceStorage
rm -rf ~/Library/Application\ Support/Firefox/Profiles/hdsrd79k.default-release/storage
rm -rf ~/Library/Caches/*
rm -rf ~/Library/Caches
rm -rf ~/Library/42_cache
rm -rf ~/Library/Safari/*
rm -rf ~/.Trash/*
rm -rf ~/.kube/cache/*
rm -fr ~/Library/Containers/com.docker.docker/Data/vms/*
rm -fr ~/Library/Containers/com.apple.Safari/Data/Library/Caches/*
rm -rfv ~/Library/Caches/*
rm -rfv ~/Library/Application\ Support/Slack/Cache/*
rm -rfv ~/Library/Application\ Support/Slack/Service\ Worker/CacheStorage/*
rm -rfv ~/Library/Group\ Containers/6N38VWS5BX.ru.keepcoder.Telegram/account-570841890615083515/postbox/*
rm -rfv ~/Library/Caches
rm -rfv ~/Library/Application\ Support/Code/Cache
rm -rfv ~/Library/Application\ Support/Code/CachedData
rm -rfv ~/Library/Application\ Support/Code/CachedExtension
rm -rfv ~/Library/Application\ Support/Code/CachedExtensions
rm -rfv ~/Library/Application\ Support/Code/CachedExtensionVSIXs
rm -rfv ~/Library/Application\ Support/Code/Code\ Cache
rm -rfv ~/Library/Application\ Support/Slack/Cache
rm -rfv ~/Library/Application\ Support/Slack/Code\ Cache
rm -rfv ~/Library/Application\ Support/Slack/Service\ Worker/CacheStorage
rm -rfv ~/Library/Application\ Support/Code/User/workspaceStorage
#calculating the new available storage after cleaning
Storage=$(df -h "$HOME" | grep "$HOME" | awk '{print($4)}' | tr 'i' 'B')
if [ "$Storage" == "0BB" ];
then
Storage="0B"
fi
sleep 1
echo "\033[32m — Available Storage After Cleaning : || $Storage || —\n\033[0m"

#df -h | grep Users | awk '{print "After:\t"$2 " = " $3 " + " $4}'

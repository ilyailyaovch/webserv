# !/bin/sh
make fclean
export VAR1
VAR1=$(date | awk '{print $2, $3, $6}')
# echo "$VAR1 $1"
git add .
git commit -m "$VAR1 $1"
if [[ $2 == 1 ]]
then
  git push
else 
  git pull
fi
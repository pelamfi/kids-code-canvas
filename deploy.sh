#!/bin/bash
set -euo pipefail
IFS=$'\n\t'
npm run webpack:production
rsync -av --delete build/ server/public/  
echo Commit files in server/public
echo and then:
echo git subtree push --prefix server heroku master

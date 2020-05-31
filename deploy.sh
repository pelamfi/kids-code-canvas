#!/bin/bash
set -euo pipefail
IFS=$'\n\t'
npm run webpack:production
rsync -nav --delete build/ server/public/  
git subtree push --prefix server heroku master

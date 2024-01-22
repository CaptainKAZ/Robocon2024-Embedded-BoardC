sudo chown -R $(whoami) .
ln -fs git-hooks .git/hooks
find . -type f -name "*.sh" -exec chmod +x {} +
find . -type f -exec dos2unix {} \;
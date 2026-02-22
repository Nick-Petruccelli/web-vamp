# Start the SSH agent if it is not already running
if [ -z "$SSH_AUTH_SOCK" ]; then
    eval "$(ssh-agent -s)"
fi
# Add your SSH key automatically (you may still be prompted for a passphrase)
ssh-add ~/.ssh/github >/dev/null 2>&1


#!/bin/bash
ssh-keyscan -p $LASDPC_PORT -H $LASDPC_IP >> ~/.ssh/known_hosts
sshpass -p "$LASDPC_PASS" ssh -p $LASDPC_PORT $LASDPC_USER@$LASDPC_IP

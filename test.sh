#!/bin/bash
./compile.sh
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'
correct=0
for i in {1..600}; do
    input="./input/input$i.txt"
    program="./bin/main $input"
    solution="./solution/output$i.txt"
    program_output="$($program)"
    trimmed_output="$(echo -n "$program_output" | sed -e 's/[[:space:]]*$//')"
    reference_content="$(cat "$solution" | sed -e 's/[[:space:]]*$//')"
    # echo "$trimmed_output"
    if [ "$trimmed_output" != "$reference_content" ]; then
        echo -e "${RED}Test case $i failed${NC}"
    else
        echo -e "${GREEN}Test case $i passed${NC}"
        correct=$((correct+1))
    fi
done
echo -e "${GREEN}Passed $correct test cases${NC} ${BLUE}$(($correct*100/600))%${NC}"
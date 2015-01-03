#!/bin/bash -f

# generates link file

echo "find . -type l -exec rm {} ';'" > bin/apply-links
find . -type l -exec bin/link.sh {} ";" >> bin/apply-links
chmod +x bin/apply-links

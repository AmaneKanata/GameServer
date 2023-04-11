#pragma once

{%- for file_name in parser.file_name %}
#include "Protocol/{{file_name}}.pb.h"
{%- endfor %}
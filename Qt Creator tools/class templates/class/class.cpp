#include "%ClassName%.%CppHeaderSuffix%"

@if "%Namespace%" != ""
namespace %Namespace%
{

@endif
@if "%BaseClassName%" != ""
@if "%TypeInfo%" != ""
%ClassName%::%ClassName%(%TypeInfo%* parent) :
    BaseClass(parent)
@else
%ClassName%::%ClassName%() :
    BaseClass()
@endif
@else
@if "%TypeInfo%" != ""
%ClassName%::%ClassName%(%TypeInfo%* parent)
@else
%ClassName%::%ClassName%()
@endif
@endif
{
}
@if "%Namespace%" != ""

} // namespace %Namespace%
@endif
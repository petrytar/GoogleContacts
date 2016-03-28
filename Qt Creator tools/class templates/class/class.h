@if "%Namespace%" != ""
#ifndef %Namespace:u%_%ClassName:u%_H
#define %Namespace:u%_%ClassName:u%_H
@else
#ifndef %ClassName:u%_H
#define %ClassName:u%_H
@endif

@if "%BaseClassName%" != ""
@if ("%BaseClassName%").charAt(0) == "Q"
#include <%BaseClassName%>

@else
#include "%BaseClassName%.%CppHeaderSuffix%"

@endif
@endif
@if "%Namespace%" != ""
namespace %Namespace%
{

@endif
/**
 * \brief The %ClassName% class.
 *
 * Add some description here.
 */
@if "%BaseClassName%" != ""
class %ClassName% : public %BaseClassName%
@else
class %ClassName%
@endif
{
@if "%TypeInfo%" != ""
    Q_OBJECT
@endif
@if "%BaseClassName%" != ""
private:
    typedef %BaseClassName% BaseClass;

@endif
public:
@if "%TypeInfo%" == ""
    %ClassName%();
@else
    %ClassName%(%TypeInfo%* parent);
@endif
};

@if "%Namespace%" != ""
} // namespace %Namespace%

@endif
@if "%Namespace%" != ""
#endif // %Namespace:u%_%ClassName:u%_H
@else
#endif // %ClassName:u%_H
@endif
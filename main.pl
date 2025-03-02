const max = 100;
var x, y;
procedure fact;
    var n, f;
    begin
        n := max;
        f := 1;
        while n > 0 do
        begin
            f := f * n;
            n := n - 1;
        end
    end;
begin
    call fact;
end.

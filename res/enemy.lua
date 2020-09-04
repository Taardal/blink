local enemy = {}

function enemy:Create()
    self.x = 123
    self.y = 666
end

function enemy:Update()
    return self.y
end

function enemy:Draw()

end

return enemy